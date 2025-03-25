const DisplayErrorHandler = require("../DisplayErrorHandler")
const SettingsManager = require("./SettingsManager")

module.exports = {
    /** @returns {{status: string, message: string, errorCode: number?}} */
    async SendCommand (userId, command) {
        const settings = await SettingsManager.LoadSettings()
        const url = "http://" + settings.info.backendAddress + "/rconSend"

        try {
            const response = await fetch(url, {
                headers: {
                    Authorization: process.env.RCON_PASSWORD,
                },
                body: JSON.stringify({ message: command }),
                method: "POST",
                signal: AbortSignal.timeout(1000)
            })
    
            const data = await response.json()
            if(data.status !== "success")
                return await DisplayErrorHandler.ParseErrMessage(userId, data)
            
            return data
        } catch (error) {
            return {
                status: "error",
                message: "Request Timeout (Backend is maybe offline?)",
                errorCode: -1
            }
        }
    }
}