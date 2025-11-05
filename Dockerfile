FROM alpine

# Build argument to enable debug mode
ARG DEBUG_BUILD=0

RUN echo "https://dl-cdn.alpinelinux.org/alpine/edge/main" >> /etc/apk/repositories && \
	echo "https://dl-cdn.alpinelinux.org/alpine/edge/testing" >> /etc/apk/repositories && \
	echo "https://dl-cdn.alpinelinux.org/alpine/edge/community" >> /etc/apk/repositories

RUN apk update && \
	apk --no-cache add make gcc g++ musl-dev curl curl-dev git cmake make ffmpeg

# Install gdb and debugging tools if DEBUG_BUILD is enabled
RUN if [ "$DEBUG_BUILD" = "1" ]; then \
		apk --no-cache add gdb bash; \
	fi

# Copy application now
WORKDIR /app
COPY ./ /app

RUN git submodule update --init --recursive

# Build with debug symbols if DEBUG_BUILD is enabled
RUN if [ "$DEBUG_BUILD" = "1" ]; then \
		cmake -S /app -B build -DDEBUG_BUILD=ON; \
	else \
		cmake -S /app -B build; \
	fi

WORKDIR /app/build
RUN make && \
	chmod +x dropout-dl && \
	cp dropout-dl ../

WORKDIR /app

# Enable core dumps for debugging crashes
RUN if [ "$DEBUG_BUILD" = "1" ]; then \
		ulimit -c unlimited || true; \
	fi

ENTRYPOINT [ "/app/dropout-dl" ]
