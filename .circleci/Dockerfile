# Jammy Jellyfish
FROM ubuntu:22.04

# Ensure default Shell is /bin/bash instead of /bin/sh/
SHELL ["/bin/bash", "-c"]

# Install GCC, Make, and Git.
RUN apt-get update && apt-get install -y \
    build-essential=12.9ubuntu3 \
    git

# Start at the Shell everytime Container runs.
ENTRYPOINT ["bash"]