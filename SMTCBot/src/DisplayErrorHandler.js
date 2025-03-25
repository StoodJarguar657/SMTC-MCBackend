const UserManager = require("./managers/UserManager")

const translation = {
    ["Wait a few seconds before the next message."]: "Please wait for 5 secconds!",
}

module.exports = {
    /**
     * @param {number} userId 
     * @param {{status: "failed"|"success", errorCode: number, message: string}} data 
     * @returns 
     */
    async ParseErrMessage(userId, data) {
        const userPermissionLevel = await UserManager.GetPermissionLevel(userId)
        if(userPermissionLevel < 2)
            return data

        console.log(`User ${userId} has triggered a error! Error: ${data}`)
        return {...data, message: translation[data.message] || "Internal Server Error"}
    }
}