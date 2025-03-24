require("dotenv").config()

const databaseManager = require("./managers/DatabaseManager")
const eventManager = require("./managers/EventManager")

const { Client, GatewayIntentBits } = require("discord.js")
const StatusManager = require("./managers/StatusManager")
const CommandManager = require("./managers/CommandManager")
const client = new Client({
    intents: [
        GatewayIntentBits.GuildPresences,
        GatewayIntentBits.GuildMembers,
        GatewayIntentBits.GuildMessages,
        GatewayIntentBits.GuildPresences,
        GatewayIntentBits.MessageContent,
    ]
})

StatusManager.Init(client)
CommandManager.Init()

eventManager.Init(client)
eventManager.CreateEvents()

client.login(process.env.TOKEN)