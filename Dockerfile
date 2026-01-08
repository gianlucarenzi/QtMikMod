# Stage 1: Build environment
FROM ubuntu:20.04 AS builder

# Docker containers run as root by default, which is suitable for this build process.
# Set environment variables for non-interactive installation
ENV DEBIAN_FRONTEND=noninteractive

# Update apt and install build tools and Qt
RUN apt-get update && \
    apt-get install -y build-essential git cmake qt5-default qttools5-dev-tools libqt5serialport5-dev patchelf libmikmod-dev curl xz-utils

# Download and extract appimagetool
RUN curl -L https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage -o /tmp/appimagetool.AppImage && \
    chmod +x /tmp/appimagetool.AppImage && \
    /tmp/appimagetool.AppImage --appimage-extract && \
    mv squashfs-root /usr/local/appimagetool_extracted && \
    rm /tmp/appimagetool.AppImage

# Download and extract linuxdeploy and plugins
RUN curl -L https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage -o /tmp/linuxdeploy.AppImage && \
    chmod +x /tmp/linuxdeploy.AppImage && \
    cd /tmp && ./linuxdeploy.AppImage --appimage-extract && \
    cp /tmp/squashfs-root/usr/bin/linuxdeploy /usr/local/bin/linuxdeploy && \
    rm -rf /tmp/squashfs-root && \
    rm /tmp/linuxdeploy.AppImage

RUN curl -L https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage -o /tmp/linuxdeploy-plugin-qt.AppImage && \
    chmod +x /tmp/linuxdeploy-plugin-qt.AppImage && \
    cd /tmp && ./linuxdeploy-plugin-qt.AppImage --appimage-extract && \
    cp /tmp/squashfs-root/usr/bin/linuxdeploy-plugin-qt /usr/local/bin/linuxdeploy-plugin-qt && \
    rm -rf /tmp/squashfs-root && \
    rm /tmp/linuxdeploy-plugin-qt.AppImage

# Copy the project source
COPY . /app
WORKDIR /app

# Build the project
RUN qmake MikModPlayer.pro
RUN make clean
RUN make -j$(nproc)

# Create a basic .desktop file for the AppImage
RUN echo "[Desktop Entry]\nName=MikModPlayer\nExec=MikModPlayer\nIcon=MikModPlayer\nType=Application\nCategories=Utility;\n" > MikModPlayer.desktop
RUN cp ./bitmaps/icon.png MikModPlayer.png

# Deploy and Package into AppImage
RUN linuxdeploy --appdir AppDir --executable ./MikModPlayer --desktop-file ./MikModPlayer.desktop --icon-file ./MikModPlayer.png --plugin qt

# Manually copy resources
RUN cp -r ./fonts AppDir/usr/bin/
RUN cp -r ./bitmaps AppDir/usr/bin/
RUN cp -r ./mod AppDir/usr/bin/

# Create custom AppRun
RUN echo '#!/bin/bash' > AppDir/AppRun && \
    echo 'HERE="$(dirname "$(readlink -f "${0}")")"' >> AppDir/AppRun && \
    echo 'cd "$HERE/usr/bin" || exit 1' >> AppDir/AppRun && \
    echo 'exec "./MikModPlayer" "$@"' >> AppDir/AppRun && \
    chmod +x AppDir/AppRun

# Convert AppDir to AppImage using appimagetool
RUN /usr/local/appimagetool_extracted/AppRun AppDir
