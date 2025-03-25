const { SlashCommandSubcommandBuilder, Events, ModalBuilder, TextInputBuilder, TextInputStyle, ActionRowBuilder } = require("discord.js");
const SettingsManager = require("../../src/managers/SettingsManager");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("update-info")
        .setDescription("Updates the current applied settings")
    ,

    async Init() { },

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const data = await SettingsManager.LoadSettings()
        const modal = new ModalBuilder()
            .setTitle("Update Settings")
            .setCustomId("updateSettings")

        const serverAddressInput = new TextInputBuilder()
            .setCustomId("serverAddress")
            .setLabel("Server Address")
            .setStyle(TextInputStyle.Short)
            .setRequired(true)
            .setValue(data.info.serverAddress)

        const mapUrl = new TextInputBuilder()
            .setCustomId("mapUrl")
            .setLabel("Map URL")
            .setStyle(TextInputStyle.Short)
            .setRequired(true)
            .setValue(data.info.mapUrl)

        const backendAddress = new TextInputBuilder()
            .setCustomId("backendAddress")
            .setLabel("Backend Address")
            .setStyle(TextInputStyle.Short)
            .setRequired(true)
            .setValue(data.info.backendAddress)

        modal.addComponents(
            new ActionRowBuilder().addComponents(serverAddressInput),
            new ActionRowBuilder().addComponents(mapUrl),
            new ActionRowBuilder().addComponents(backendAddress)
        )

        await interaction.showModal(modal);
    }
}