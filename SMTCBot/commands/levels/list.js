import { MessageFlags, SlashCommandSubcommandBuilder } from "discord.js"

export default {
    data: new SlashCommandSubcommandBuilder()
        .setName("list")
        .setDescription("Lists all registered users in the database"),

    permissionLevel: 1,

    async init() {},

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        await interaction.reply({ content: "Hello world!", flags: MessageFlags.Ephemeral })
    }
}