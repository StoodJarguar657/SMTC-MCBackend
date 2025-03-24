const fs = require('fs')

module.exports = {
    /** @type {import("discord.js").Client?}  */
    client: null,

    /**
     * @param {import("discord.js").Client} client 
     */
    Init(client) {
        console.log("EventManager.Init")
        
        this.client = client
    },

    CreateEvents() {
        const eventsDirectory = fs.readdirSync("events").filter(file => file.endsWith(".js"))
        
        for(const eventFilename of eventsDirectory) {
            const eventName = eventFilename.slice(null, -3)
            const eventCallback = require(`../../events/${eventFilename}`)
            this.client.on(eventName, eventCallback)

            console.log(`Registered event ${eventFilename.slice(null, -3)}`)
        }
    }
}