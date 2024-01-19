#!/home/leo_zhang/miniconda3/envs/web_auto/bin/python

import os
import requests
import platform
from datetime import datetime


def download_pic_of_the_day():
    PIC_COLLECTION_NAME = "nasa_pic_collection"
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
        json_output["date"],
        json_output["explanation"],
        json_output["hdurl"],
        json_output["media_type"],
        json_output["service_version"],
        json_output["title"],
        json_output["url"],
        json_output["copyright"],
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
    file_directory = (
        "C:\\Users\\{}\\Documents\\GitHub\\Tools\\python_tools\\fetch_nasa_pic".format(
            os.environ.get("USERNAME")
        )
        if platform.system() == "Windows"
        else (
            "/home/{}/Documents/GitHub/Tools/python_tools/fetch_nasa_pic/".format(
                os.environ.get("USER")
            )
            if platform.system() == "Linux"
            else ""
        )
    )

    if not os.path.exists(file_directory):
        print(f'"{file_directory}" does not exist')
    else:
        os.chdir(file_directory)
        download_pic_of_the_day()
