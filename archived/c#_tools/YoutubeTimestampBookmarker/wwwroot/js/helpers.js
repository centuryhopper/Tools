window.getCurrentTabUrl = async () => {
    return new Promise((resolve) => {
        chrome.tabs.query({ active: true, currentWindow: true }, (tabs) => {
            resolve(tabs[0].url);
        });
    });
};

window.openIndexPage = () => {
    chrome.tabs.create({ url: chrome.runtime.getURL("index.html") });
}

window.getVideoTime = async () => {
    return new Promise((resolve) => {
        chrome.scripting.executeScript(
            {
                target: { tabId: chrome.devtools.inspectedWindow.tabId },
                func: () => {
                    const video = document.querySelector("video");
                    return video ? Math.floor(video.currentTime) : 0;
                }
            },
            (results) => {
                resolve(results?.[0]?.result ?? 0);
            }
        );
    });
};

window.saveTimestamps = (data) => {
    chrome.storage.local.set({ yt_timestamps: data });
};

window.loadTimestamps = () => {
    return new Promise((resolve) => {
        chrome.storage.local.get("yt_timestamps", (result) => {
            if (result.yt_timestamps)
            {
                result.yt_timestamps = JSON.parse(result.yt_timestamps)
                // console.log(result.yt_timestamps);
            }
            resolve(result.yt_timestamps || []);
        });
    });
};
