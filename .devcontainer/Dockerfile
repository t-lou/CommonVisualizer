FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
  && apt-get install -y --no-install-recommends \
    sudo git build-essential cmake pkg-config curl wget xauth \
    libx11-dev libgl1-mesa-dev libglu1-mesa-dev libglm-dev libglew-dev libglfw3-dev libpng++-dev freeglut3-dev libeigen3-dev \
    locales ca-certificates tzdata \
  && locale-gen en_US.UTF-8 \
  && rm -rf /var/lib/apt/lists/*

ARG USERNAME=docker
ARG HOST_UID=1000
ARG HOST_GID=1000
RUN if getent passwd $HOST_UID; then \
        usermod -l $USERNAME -d /home/$USERNAME -m $(getent passwd $HOST_UID | cut -d: -f1); \
    else \
        groupadd -g $HOST_GID $USERNAME && \
        useradd -m -u $HOST_UID -g $HOST_GID -s /bin/bash $USERNAME; \
    fi

WORKDIR /workspace
USER $USERNAME
CMD ["bash"]

# Command build:
# docker compose -f .devcontainer/docker-compose.yml up --build -d