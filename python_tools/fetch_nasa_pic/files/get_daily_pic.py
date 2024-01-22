#!/home/leo_zhang/miniconda3/envs/web_auto/bin/python

import os
import requests
import platform
from datetime import datetime

os.chdir(os.path.dirname(__file__))

PIC_COLLECTION_NAME = "nasa_pic_collection"


def download_pic_of_the_day():
    pic_collection_path = os.path.join(os.getcwd(), PIC_COLLECTION_NAME)
    today_string = datetime.now().strftime("%Y-%m-%d")
    today_folder_name = os.path.join(pic_collection_path, today_string)

    if not os.path.exists(pic_collection_path):
        os.makedirs(pic_collection_path)

    if os.path.exists(today_folder_name):
        print("already gathered today's image")
        return

    os.makedirs(today_folder_name)

    url = "https://api.nasa.gov/planetary/apod?api_key=DEMO_KEY"
    response = requests.get(url)

    if not response.status_code == 200:
        raise Exception(f"Error: {response.status_code}")

    json_output = response.json()
    # print(json_output)

    date, explanation, hdurl, media_type, service_version, title, url, copyright = (
        json_output.get("date", "n/a"),
        json_output.get("explanation", "n/a"),
        json_output.get("hdurl", "n/a"),
        json_output.get("media_type", "n/a"),
        json_output.get("service_version", "n/a"),
        json_output.get("title", "n/a"),
        json_output.get("url", "n/a"),
        # some times there's no copyright available
        json_output.get("copyright", "n/a"),
    )

    filename = os.path.join(today_folder_name, f"{today_string}.txt")
    with open(filename, "w") as file:
        file.write(
            "Date: {}\n\nExplanation: {}\n\nHdurl: {}\n\nMedia_type: {}\n\nService_version: {}\n\nTitle: {}\n\nUrl: {}\n\nCopyright: {}".format(
                date,
                explanation,
                hdurl,
                media_type,
                service_version,
                title,
                url,
                copyright,
            )
        )

    image_url = json_output["hdurl"]
    image_response = requests.get(image_url)

    if image_response.status_code == 200:
        pic_filename = f"nasa_pic_{today_string}.jpg"
        with open(os.path.join(today_folder_name, pic_filename), "wb") as file:
            file.write(image_response.content)


if __name__ == "__main__":
    # print(os.getcwd())
    file_directory = os.path.join(os.getcwd(), PIC_COLLECTION_NAME)

    if not os.path.exists(file_directory):
        print(f'"{file_directory}" does not exist, so creating it now...')
        os.makedirs(file_directory)

    print(os.getcwd())
    # os.chdir(file_directory)
    download_pic_of_the_day()

