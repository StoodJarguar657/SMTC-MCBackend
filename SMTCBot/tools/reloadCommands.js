import { configDotenv } from "dotenv";
configDotenv()

import commandManager from "../src/commandManager.js";
import { REST, Routes } from "discord.js";

async function run() {
    await commandManager.init()
    const jsonCommands = commandManager.commands.map(command => command.data.toJSON())

    const rest = new REST().setToken(process.env.TOKEN);
    await rest.put(Routes.applicationGuildCommands(process.env.CLIENT_ID, process.env.GUILD_ID), { body: jsonCommands })

    console.log("Refreshed commands")
}

run()