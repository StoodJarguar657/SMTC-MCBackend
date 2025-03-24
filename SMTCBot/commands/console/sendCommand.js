const { SlashCommandSubcommandBuilder, MessageFlags } = require("discord.js");
const { randomUUID } = require("crypto");

const RCONManager = require("../../src/managers/RCONManager");
const fs = require("fs/promises");

module.exports = {
    permissionLevel: 1,
    data: new SlashCommandSubcommandBuilder()
        .setName("sendcommand")
        .setDescription("Sends a command")
        .addStringOption(option => option.setName("command").setDescription("The command to run").setRequired(true)),

    async Init() {},

    /** @param {import("discord.js").Interaction} interaction */
    async Execute(interaction) {
        const response = await RCONManager.SendCommand(interaction.options.getString("command"))
        if(response.status === "success") {
            if(response.message.length === 0)
                return await interaction.reply({ content: "\`\`\`\nNo response received\n\`\`\`", flags: MessageFlags.Ephemeral})

            if(response.message.length <= 2000)
                return await interaction.reply({ content: `\`\`\`\n${response.message}\n\`\`\``, flags: MessageFlags.Ephemeral})

            const filePath = `temp/${randomUUID()}.txt`
            await fs.writeFile(filePath, response.message, "utf8")

            await interaction.reply({ files: [filePath], flags: MessageFlags.Ephemeral })

            await fs.rm(filePath)
            return
        }

        return await interaction.reply({ content: `Failed to send RCON message! ${response.message}`, flags: MessageFlags.Ephemeral })
    }
}