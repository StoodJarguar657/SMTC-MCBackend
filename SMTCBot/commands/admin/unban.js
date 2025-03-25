const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const RCONManager = require("../../src/managers/RCONManager");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("unban")
        .setDescription("Unbans a player")
        .addStringOption(option => option.setName("username").setDescription("The name of the user to unban").setRequired(true))
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const username = interaction.options.getString("username")
        const response = await RCONManager.SendCommand(interaction.member.id, `/pardon ${username}`)

        await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
    }
}