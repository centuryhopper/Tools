FROM mcr.microsoft.com/dotnet/sdk:8.0 AS build

ARG BUILDCONFIG=RELEASE

# Copy project files to avoid restoring packages if they haven't changed
COPY *.csproj ./build/
WORKDIR /build/
RUN dotnet restore

COPY . .
RUN dotnet publish -c $BUILDCONFIG -o out

# build runtime image
# arm64v8 for Raspberry PIs
FROM mcr.microsoft.com/dotnet/aspnet:8.0.1-bookworm-slim-arm64v8
#FROM mcr.microsoft.com/dotnet/aspnet:8.0
WORKDIR /app
COPY --from=build /build/out ./

# Install packages
RUN apt-get update && \
  apt-get -y install cron tree vim less wget unzip tzdata firefox-esr

# Download and set up geckodriver
RUN wget https://github.com/mozilla/geckodriver/releases/download/v0.35.0/geckodriver-v0.35.0-linux-aarch64.tar.gz && \
    tar -xvzf geckodriver-v* && \
    chmod +x geckodriver && \
    mv geckodriver /usr/local/bin/ && \
    rm geckodriver-v*


# Add export environment variable script and schedule.txt
COPY ./files/*.sh ./
COPY ./files/schedule.txt /etc/cron.d/schedule.txt

# destination directories must end with a '/'
COPY ./configs/config.json ./configs/

# remove carriage returns to make sure this works on windows machines
RUN sed -i 's/\r//' export_env.sh \
  && sed -i 's/\r//' run_app.sh \
  && sed -i 's/\r//' /etc/cron.d/schedule.txt \
  && chmod +x export_env.sh run_app.sh \
  && chmod 0644 /etc/cron.d/schedule.txt

RUN /usr/bin/crontab /etc/cron.d/schedule.txt

# Create log file
RUN touch /var/log/cron.log
RUN chmod 0666 /var/log/cron.log


# Run Cron
CMD /app/export_env.sh && cron && tail -f /var/log/cron.log



