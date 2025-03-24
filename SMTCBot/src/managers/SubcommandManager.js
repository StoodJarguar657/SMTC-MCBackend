const path = require("path")
const fs = require("fs")

module.exports = {
    
    /** @param {{data: import("discord.js").SlashCommandBuilder;Init(): Promise<void>;Execute(interaction: import("discord.js").Interaction): Promise<void>;}} data */
    async InitCommand(data, fileName) {
        const filename = path.basename(fileName).slice(0, -3)

        const subCommandsDirectoryPath = path.join(path.dirname(fileName), filename)
        const subCommandsDirectory = fs.readdirSync(subCommandsDirectoryPath).filter(file => file.endsWith(".js"))

        data.subCommands = []

        for(const subCommandFile of subCommandsDirectory) {
            const filepath = path.join(subCommandsDirectoryPath, subCommandFile)

            /** @type {{data: import("discord.js").SlashCommandSubcommandBuilder; Init(): Promise<void>;Execute(interaction: import("discord.js").Interaction): Promise<void>;}} */
            const subCommand = require(filepath)
            subCommand.Init()

            data.data.addSubcommand(subCommand.data)
            data.subCommands[subCommand.data.name] = subCommand
        }
    },

    /**
     * @param {import("discord.js").Interaction} interaction 
     */
    async RunCommand(data, interaction) {
        const subCommand = interaction.options.getSubcommand()
        if(!subCommand)
            return await interaction.reply("Failed to get subcommand!")

        /** @type {{data: import("discord.js").SlashCommandSubcommandBuilder; Init(): Promise<void>;Execute(interaction: import("discord.js").Interaction): Promise<void>;}?} */
        const subcommand = data.subCommands[subCommand]
        if(!subcommand)
            return await interaction.reply("Unknown subcommand!")

        return await subcommand.Execute(interaction)
    }
}