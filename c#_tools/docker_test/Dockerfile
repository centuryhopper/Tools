FROM mcr.microsoft.com/dotnet/sdk:8.0 AS build

ARG BUILDCONFIG=RELEASE

# Copy project files to avoid restoring packages if they haven't changed
COPY *.csproj ./build/
WORKDIR /build/
RUN dotnet restore

COPY . .
RUN dotnet publish -c $BUILDCONFIG -o out

# build runtime image
FROM mcr.microsoft.com/dotnet/aspnet:8.0
WORKDIR /app
COPY --from=build /build/out ./

# Install Cron
RUN apt-get update -qq && apt-get -y install cron tree vim less -qq --force-yes

# Add export environment variable script and schedule.txt
COPY *.sh ./
COPY schedule.txt /etc/cron.d/schedule.txt

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

# Volume required for tail command
VOLUME /var/log

# Run Cron
CMD /app/export_env.sh && cron && tail -f /var/log/cron.log
