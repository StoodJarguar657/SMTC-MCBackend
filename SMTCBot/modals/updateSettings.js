const { MessageFlags } = require("discord.js")
const SettingsManager = require("../src/managers/SettingsManager")

/**
 * @param {import("discord.js").Interaction} interaction 
 */
module.exports = async (interaction) => {
    const settings = await SettingsManager.LoadSettings()
    settings.info.serverAddress = interaction.fields.getTextInputValue("serverAddress")
    settings.info.mapUrl = interaction.fields.getTextInputValue("mapUrl")
    settings.info.backendAddress = interaction.fields.getTextInputValue("backendAddress")
    
    await SettingsManager.SaveSettings(settings)
    return await interaction.reply({ content: "Applied new settings!", flags: MessageFlags.Ephemeral })
}