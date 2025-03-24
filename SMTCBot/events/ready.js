const StatusManager = require("../src/managers/StatusManager")

/**
 * @param {import("discord.js").Client} readyClient 
 */
module.exports = (readyClient) => {
    console.log(`Logged in as ${readyClient.user.tag}`)

    StatusManager.Run()
}