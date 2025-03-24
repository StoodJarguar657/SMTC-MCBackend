const fs = require("fs/promises")
const path = require("path")

const UserManager = require("./UserManager")

const { MessageFlags } = require("discord.js")

const { Routes } = require("discord-api-types/v9")
const { REST } = require("@discordjs/rest")


module.exports = {
    async Init() {
        const commands = await this.GetCommands()
        for(const command of commands) {
            command.Init()
        }
    },

    /**
     * @param {import("discord.js").Interaction} interaction 
     */
    async RunCommand(interaction) {
        const userPermissionLevel = await UserManager.GetPermissionLevel(interaction.member.id)
        const commands = await this.GetCommands()

        for(const command of commands) {
            const commandName = command.data.name
            if(commandName !== interaction.commandName)
                continue

            if(command.permissionLevel > userPermissionLevel)
                return await interaction.reply({ content: "You do not have the permissions in-order to run this command!", flags: MessageFlags.Ephemeral })

            return await command.Execute(interaction)
        }
        
        // Its a command we don't have permission, respond with a permission error.

        await interaction.reply({ content: "Unknown command", flags: MessageFlags.Ephemeral })
    },

    async GetCommands() {
        /**@type {{permissionLevel: number; data: import('discord.js').SlashCommandBuilder; Init(): Promise<void>; Execute(interaction: import("discord.js").Interaction): Promise<void>;}[]} */
        const commands = []
        const commandDirectory = (await fs.readdir("commands")).filter(file => file.endsWith(".js"))

        for(const command of commandDirectory)
            commands.push(require(`../../commands/${command}`))

        return commands
    },

    async ReloadCommands() {
        const commands = (await this.GetCommands()).map(command => command.data)
        const rest = new REST({ version: "9" }).setToken(process.env.TOKEN)

        try {
            const existingCommands = await rest.get(Routes.applicationCommands(process.env.CLIENT_ID))
            for (const command of existingCommands) {
                await rest.delete(Routes.applicationCommand(process.env.CLIENT_ID, command.id))
            }

            await rest.put(Routes.applicationCommands(process.env.CLIENT_ID), { body: commands.map(command => command.toJSON()) })
            return true
        } catch (error) {
            console.error(error)
            return false
        }
    },

    async DeleteCommands() {
        const rest = new REST({ version: "9" }).setToken(process.env.TOKEN)

        try {
            const existingCommands = await rest.get(Routes.applicationCommands(process.env.CLIENT_ID))
            for (const command of existingCommands) {
                await rest.delete(Routes.applicationCommand(process.env.CLIENT_ID, command.id))
            }

            return true
        } catch (error) {
            console.error(error)
            return false
        }
    },

    async ReloadGuildCommands(guildId) {
        const commands = (await this.GetCommands()).map(command => command.data)
        const rest = new REST({ version: "9" }).setToken(process.env.TOKEN)

        try {
            const existingCommands = await rest.get(Routes.applicationGuildCommands(process.env.CLIENT_ID, guildId))
            for (const command of existingCommands) {
                await rest.delete(Routes.applicationGuildCommand(process.env.CLIENT_ID, guildId, command.id))
            }

            await rest.put(Routes.applicationGuildCommands(process.env.CLIENT_ID, guildId), { body: commands.map(command => command.toJSON()) })
            return true
        } catch (error) {
            console.error(error)
            return false
        }
    },

    async DeleteGuildCommands(guildId) {
        const rest = new REST({ version: "9" }).setToken(process.env.TOKEN)

        try {
            const existingCommands = await rest.get(Routes.applicationGuildCommands(process.env.CLIENT_ID, guildId))
            for (const command of existingCommands) {
                await rest.delete(Routes.applicationGuildCommand(process.env.CLIENT_ID, guildId, command.id))
            }

            return true
        } catch (error) {
            console.error(error)
            return false
        }
    }
}