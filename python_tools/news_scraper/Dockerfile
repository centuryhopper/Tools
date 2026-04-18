# Use the official Python image for ARM64 architecture
FROM --platform=linux/arm64/v8 python:3.11-slim

ENV PYTHONUNBUFFERED=1
RUN TZ=America/New_York && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get -y update && apt-get -y install cron tree vim less


# IMPORTANT: Make sure start.sh is executable 
COPY files/* /home/

# install any dependencies
RUN pip install --no-cache-dir -r /home/requirements.txt

#RUN chmod 755 /script.sh /entry.sh
RUN /usr/bin/crontab /home/crontab.txt

# Run the command on container startup
RUN touch /var/log/cron.log


CMD ["/bin/sh", "-c", "cron -f"]




