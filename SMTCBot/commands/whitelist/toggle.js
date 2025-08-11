import { MessageFlags, SlashCommandSubcommandBuilder } from "discord.js"
import serverManager from "../../src/serverManager.js"

export default {
    data: new SlashCommandSubcommandBuilder()
        .setName("toggle")
        .setDescription("Enables/disables whitelist")
        .addBooleanOption(option => option.setName("enabled").setDescription("To enable or disable whitelist").setRequired(true)),

    permissionLevel: 2,

    async init() { },

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const enabled = interaction.options.getBoolean("enabled")
        const response = await serverManager.sendRcon(serverInfo, `whitelist ${enabled ? "on" : "off"}`)

        if (response.status === "success") {
            return await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral})
        }

        await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
    }
}