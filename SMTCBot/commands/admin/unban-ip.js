const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const RCONManager = require("../../src/managers/RCONManager");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("unban-ip")
        .setDescription("Unbans a IP")
        .addStringOption(option => option.setName("ip").setDescription("The ip to unban").setRequired(true))
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const username = interaction.options.getString("ip")        
        const response = await RCONManager.SendCommand(`/pardon-ip ${username}`)

        await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
    }
}