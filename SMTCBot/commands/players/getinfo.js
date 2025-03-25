const { SlashCommandSubcommandBuilder, MessageFlags, ButtonBuilder, ActionRowBuilder, ButtonStyle } = require("discord.js");
const RCONManager = require("../../src/managers/RCONManager");
const SNBT = require("../../src/snbt/SNBT");

const SettingsManager = require("../../src/managers/SettingsManager");

let worldNameTable = {
    ["minecraft:overworld"]: "Overworld",
    ["minecraft:the_nether"]: "Nether",
    ["minecraft:the_end"]: "End",
}

let gamemodeTable = [
    "Survival",
    "Creative",
    "Adventure",
    "Spectator"
]

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("getinfo")
        .setDescription("Gets information about a player")
        .addStringOption(option => option.setName("username").setDescription("The user to get info out from.").setRequired(true))
    ,

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const settings = SettingsManager.LoadSettings()

        /** @type {string} */
        const username = interaction.options.getString("username")
        if(username.startsWith("@"))
            return await interaction.reply({ content: "Invalid username!", flags: MessageFlags.Ephemeral })

        const response = await RCONManager.SendCommand(`/data get entity ${username}`)
        if(response.status !== "success")
            return await interaction.reply({ content: `Failed to receive information! ${response.message}`, flags: MessageFlags.Ephemeral })

        if(response.message === "No entity was found")
            return await interaction.reply

        let message = `**Information from \`${username}\`\n\n**`
        const snbtDataText = response.message.slice(username.length + " has the following entity data: ".length)
        const data = await SNBT.Parse(snbtDataText)

        message += `Position: \`(X: ${Math.floor(data.Pos[0])}, Y: ${Math.floor(data.Pos[1])}, Z: ${Math.floor(data.Pos[2])})\`\n`
        message += `Dimension: \`${worldNameTable[data.Dimension] || "Unknown"}\`\n`
        message += `-------------------------------------------------------\n`
        message += `Health: **${data.Health}/20**\n`
        message += `Food: **${data.foodLevel}/20**\n`
        message += `XP Level: **${data.XpLevel || 0}**\n`
        message += `-------------------------------------------------------\n`
        message += `Gamemode **${gamemodeTable[data.playerGameType] || "Unknown"}**\n`

        const openNameMcBtn = new ButtonBuilder()
            .setLabel("Open NameMC Skin")
            .setURL(`https://namemc.com/profile/${username}`)
            .setStyle(ButtonStyle.Link)
        
        const row = new ActionRowBuilder()
			.addComponents(openNameMcBtn);

        return await interaction.reply({ content: message, flags: MessageFlags.Ephemeral, components: [row] })
    }
}