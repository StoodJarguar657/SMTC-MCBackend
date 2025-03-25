const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const RCONManager = require("../../src/managers/RCONManager");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("remove")
        .setDescription("Removes a player to the whitelist")
        .addStringOption(option => option.setName("username").setDescription("The name of the user to remove").setRequired(true))
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const username = interaction.options.getString("username")
        const response = await RCONManager.SendCommand(interaction.member.id, `/whitelist remove ${username}`)

        await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
    }
}