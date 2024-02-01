FROM --platform=linux/arm64/v8 mcr.microsoft.com/dotnet/sdk:8.0 AS build-env
WORKDIR /App

# Copy everything
COPY . ./
# Restore as distinct layers
RUN dotnet restore
# Build and publish a release
RUN dotnet publish -c Release -o out

# Build runtime image
FROM mcr.microsoft.com/dotnet/aspnet:8.0
WORKDIR /App
COPY --from=build-env /App/out .
ENTRYPOINT ["dotnet", "DotNet.Docker.dll"]

RUN TZ=America/New_York && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get -y update && apt-get -y install cron tree vim less


# IMPORTANT: Make sure start.sh is executable 
COPY files/* /home/

# install any dependencies

#RUN chmod 755 /script.sh /entry.sh
RUN /usr/bin/crontab /home/crontab.txt

# Run the command on container startup
RUN touch /var/log/cron.log

CMD cron && tail -f /var/log/cron.log






# docker build -t get_nasa_pic_cron .
# docker push leozhang1/automation:get_nasa_pic_cron
# docker run leozhang1/automation:get_nasa_pic_cron

# docker run -itd get_nasa_pic_cron
