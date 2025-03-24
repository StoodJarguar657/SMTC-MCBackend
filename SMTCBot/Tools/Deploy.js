require("dotenv").config()

const CommandManager = require("../src/managers/CommandManager")
CommandManager.Init()
CommandManager.ReloadGuildCommands(process.env.GUILD_ID)