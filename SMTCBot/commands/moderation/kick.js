import { MessageFlags, SlashCommandSubcommandBuilder } from "discord.js"
import serverManager from "../../src/serverManager.js"

export default {
    data: new SlashCommandSubcommandBuilder()
        .setName("kick")
        .setDescription("Kicks a user")
        .addStringOption(option => option.setName("username").setDescription("The user to kick").setRequired(true))
        .addStringOption(option => option.setName("reason").setDescription("The reason for the kick").setRequired(false)),

    permissionLevel: 2,

    async init() {},

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const username = interaction.options.getString("username")
        const reason   = interaction.options.getString("reason") || "Kicked by a administrator"
        
        const response = await serverManager.sendRcon(serverInfo, `kick ${username} ${reason}`)
        if (response.status === "success") {
            return await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral})
        }

        await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
    }
}