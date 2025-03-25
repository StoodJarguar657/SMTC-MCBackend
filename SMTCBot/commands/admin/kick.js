const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const RCONManager = require("../../src/managers/RCONManager");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("kick")
        .setDescription("Kicks a player")
        .addStringOption(option => option.setName("username").setDescription("The name of the user to kick").setRequired(true))
        .addStringOption(option => option.setName("reason").setDescription("The reason for the kick").setRequired(false))
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const username = interaction.options.getString("username")
        const reason = interaction.options.getString("reason") || "No reason specified!"
        
        const response = await RCONManager.SendCommand(interaction.member.id, `/kick ${username} ${reason}`)

        await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
    }
}