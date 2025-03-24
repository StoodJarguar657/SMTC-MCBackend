require("dotenv").config()

const CommandManager = require("../src/managers/CommandManager")
CommandManager.Init()
CommandManager.ReloadCommands(process.env.GUILD_ID)