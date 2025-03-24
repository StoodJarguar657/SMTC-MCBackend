const { ActivityType } = require("discord.js")
const RequireNoCache = require("../RequireNoCache")

module.exports = {
    /** @type {import("discord.js").Client?}  */
    client: null,

    currentStatusIndex: 0,

    /**
     * @param {import("discord.js").Client} client 
     */
    Init(client) {
        console.log("StatusManager.Init")
        this.client = client
    },

    Run() {
        this.Loop()
        setInterval(() => this.Loop(), 5000)
    },

    Loop() {
        try {
            const statusMessages = RequireNoCache("../../statusMessages.json")
            if(this.currentStatusIndex >= statusMessages.messages.length) {
                this.currentStatusIndex = 0
            }

            const statusMessage = statusMessages.messages[this.currentStatusIndex]

            this.client.user.setActivity(statusMessage, { type: ActivityType.Custom })
            this.currentStatusIndex++
        } catch (error) {}
    }
}