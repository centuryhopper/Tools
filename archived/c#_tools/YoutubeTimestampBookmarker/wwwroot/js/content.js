

(function () {

    // console.log('Content script loaded!');
    if (document.URL.includes("youtube.com/watch"))
    {
        // create and add floating button
        if (document.getElementById("yt-save-button")) return; // Avoid duplicates

        const button = document.createElement("button");
        button.innerText = "💾 Save Timestamp";
        button.id = "yt-save-button";

        // Style the floating button
        Object.assign(button.style, {
            position: "fixed",
            bottom: "100px",
            right: "20px",
            zIndex: 9999,
            padding: "10px 16px",
            backgroundColor: "#FF0000",
            color: "white",
            fontSize: "14px",
            border: "none",
            borderRadius: "8px",
            cursor: "pointer",
        });

        button.onclick = async () => {
            const video = document.querySelector("video");
            const time = Math.floor(video?.currentTime || 0);
            const url = new URL(window.location.href);
            const title = document.title;

            url.searchParams.set("t", `${time}s`);

            const entry = {
                label: `⏱️ ${title} @ ${time}s`,
                url: url.toString()
            };

            // Get existing
            // const existing = JSON.parse(localStorage.getItem("yt_timestamps") || "[]");
            const lstOfTimeStamps = await window.loadTimestamps()
            // console.log('lstOfTimeStamps: ' + lstOfTimeStamps);
            // console.log(lstOfTimeStamps === undefined);
            // console.log(lstOfTimeStamps.length);

            lstOfTimeStamps.push(entry);

            // Save it
            // localStorage.setItem("yt_timestamps", JSON.stringify(existing));
            window.saveTimestamps(JSON.stringify(lstOfTimeStamps))

            // Optional feedback
            button.innerText = "✅ Saved!";
            setTimeout(() => button.innerText = "💾 Save Timestamp", 1000);
        };

        document.body.appendChild(button);
    }
    
})();