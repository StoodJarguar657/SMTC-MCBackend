import { MessageFlags, SlashCommandSubcommandBuilder } from "discord.js"
import serverManager from "../../src/serverManager.js"

export default {
    data: new SlashCommandSubcommandBuilder()
        .setName("op")
        .setDescription("Gives operator permissions to a user")
        .addStringOption(option => option.setName("username").setDescription("The user to give it to").setRequired(true)),

    permissionLevel: 2,

    async init() {},

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const username = interaction.options.getString("username")        
        const response = await serverManager.sendRcon(serverInfo, `op ${username}`)
        if (response.status === "success") {
            return await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral})
        }

        await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
    }
}