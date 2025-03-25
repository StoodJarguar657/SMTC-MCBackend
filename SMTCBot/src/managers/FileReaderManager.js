const DisplayErrorHandler = require("../DisplayErrorHandler")
const SettingsManager = require("./SettingsManager")

module.exports = {
    async ReadFileFromServerJSON(userId, filePath) {
        try {
            const settings = await SettingsManager.LoadSettings()
            const response = await fetch("http://" + settings.info.backendAddress + "/readFile", {
                headers: {
                    Authorization: process.env.RCON_PASSWORD,
                },
                method: "POST",
                body: JSON.stringify({ filePath: filePath }),
                signal: AbortSignal.timeout(1000)
            })

            const data = await response.json()
            if(data.status !== "success")
                return await DisplayErrorHandler.ParseErrMessage(userId, data)
            
            return {
                status: "success",
                data: data
            }
        } catch (error) {
            return {
                status: "error",
                message: "Request Timeout (Backend is maybe offline?)",
                errorCode: -1
            }
        }
    }
}