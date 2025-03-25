const SettingsManager = require("./SettingsManager")

module.exports = {
    async ReadFileFromServerText(filePath) {
        const settings = await SettingsManager.LoadSettings()
        const response = await fetch("http://" + settings.info.backendAddress + "/readFile", {
            headers: {
                Authorization: process.env.RCON_PASSWORD,
            },
            method: "POST",
            body: JSON.stringify({ filePath: filePath })
        })

        return await response.text()
    },

    async ReadFileFromServerJSON(filePath) {
        const settings = await SettingsManager.LoadSettings()
        const response = await fetch("http://" + settings.info.backendAddress + "/readFile", {
            headers: {
                Authorization: process.env.RCON_PASSWORD,
            },
            method: "POST",
            body: JSON.stringify({ filePath: filePath })
        })

        return await response.json()
    }
}