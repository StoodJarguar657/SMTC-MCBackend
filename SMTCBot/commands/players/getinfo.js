const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const RCONManager = require("../../src/managers/RCONManager");

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
        const username = interaction.options.getString("username")
        if(username.startsWith("@"))
            return await interaction.reply({ content: "Invalid username!", flags: MessageFlags.Ephemeral })

        const response = await RCONManager.SendCommand(`/data get entity ${username}`)
        if(response.status !== "success")
            return await interaction.reply({ content: `Failed to receive information! ${response.message}`, flags: MessageFlags.Ephemeral })

        if(response.message === "No entity was found")
            return await interaction.reply

        let message = `**Information from \`${username}\`**`
        const jsonDataText = response.message.slice(response.message.length + " has the following entity data: ".length)
        console.log(jsonDataText)

        
        return await interaction.reply({ content: message, flags: MessageFlags.Ephemeral })
    }
}