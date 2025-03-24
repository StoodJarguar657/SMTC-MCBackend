const { SlashCommandBuilder, MessageFlags } = require("discord.js");
const RCONManager = require("../src/managers/RCONManager");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandBuilder()
        .setName("seed")
        .setDescription("Shows you the world seed"),

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const response = await RCONManager.SendCommand("/seed")
        if(response.status !== "success")
            return await interaction.reply({ content: "Failed to get seed!", flags: MessageFlags.Ephemeral })
        
        await interaction.reply({ content: `**Server Seed:** \`${response.message.slice(7, -1)}\``, flags: MessageFlags.Ephemeral})
    }
}