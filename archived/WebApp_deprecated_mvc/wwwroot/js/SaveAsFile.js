function saveAsFile(filename, fileContent)
{
    var link = document.createElement('a')
    link.download = filename
    // mime-types: https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
    link.href = 'data:text/plain;charset=utf-8,'+encodeURIComponent(fileContent)
    document.body.appendChild(link)
    link.click()
    document.body.removeChild(link)
}

