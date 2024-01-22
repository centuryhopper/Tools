# Use the official Python image for ARM64 architecture
FROM --platform=linux/arm64/v8 python:rc-slim-buster


RUN TZ=America/New_York && ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get -y update && apt-get -y install cron

COPY files/* /home/

# install any dependencies
RUN pip install --no-cache-dir -r /home/requirements.txt

#RUN chmod 755 /script.sh /entry.sh
RUN /usr/bin/crontab /home/crontab.txt

# Run the command on container startup
RUN touch /var/log/cron.log

CMD cron && tail -f /var/log/cron.log






# docker build -t get_nasa_pic_cron .
# docker push leozhang1/automation:get_nasa_pic_cron
# docker run leozhang1/automation:get_nasa_pic_cron

# docker run -itd get_nasa_pic_cron
