const { SlashCommandBuilder } = require("discord.js");
const SettingsManager = require("../src/managers/SettingsManager");

module.exports = {
    permissionLevel: 0,
    data: new SlashCommandBuilder()
        .setName("server-info")
        .setDescription("Gives you server information"),

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const settings = await SettingsManager.LoadSettings()

        await interaction.reply(`**Information about the server:**\n\nServer Address: **${settings.info.serverAddress}**\nMap URL: **${settings.info.mapUrl}**`)
    }
}