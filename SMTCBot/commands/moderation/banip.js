import { MessageFlags, SlashCommandSubcommandBuilder } from "discord.js"
import serverManager from "../../src/serverManager.js"

export default {
    data: new SlashCommandSubcommandBuilder()
        .setName("ban-ip")
        .setDescription("Bans a ip address")
        .addStringOption(option => option.setName("ip").setDescription("The ip address to ban").setRequired(true))
        .addStringOption(option => option.setName("reason").setDescription("The reason for the ban").setRequired(false)),

    permissionLevel: 2,

    async init() {},

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const ip = interaction.options.getString("ip")
        const reason   = interaction.options.getString("reason") || "IP-Banned by a administrator"
        
        const response = await serverManager.sendRcon(serverInfo, `ban-ip ${ip} ${reason}`)
        if (response.status === "success") {
            return await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral})
        }

        await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
    }
}