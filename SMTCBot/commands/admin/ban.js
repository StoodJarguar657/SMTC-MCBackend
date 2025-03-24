const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const RCONManager = require("../../src/managers/RCONManager");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("ban")
        .setDescription("Bans a player")
        .addStringOption(option => option.setName("username").setDescription("The name of the user to ban").setRequired(true))
        .addStringOption(option => option.setName("reason").setDescription("The reason for the ban").setRequired(false))
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const username = interaction.options.getString("username")
        const reason = interaction.options.getString("reason") || "No reason specified!"
        
        const response = await RCONManager.SendCommand(`/ban ${username} ${reason}`)

        await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
    }
}