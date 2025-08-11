import { MessageFlags, SlashCommandBuilder } from "discord.js"

export default {
    data: new SlashCommandBuilder()
        .setName("info")
        .setDescription("Provides server information"),

    permissionLevel: 1,

    async init() {},

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        await interaction.reply({ content: `# ${serverInfo.displayName}'s Information\nServer IP: ${serverInfo.address}\n\n${serverInfo.description}`, flags: MessageFlags.Ephemeral })
    }
}