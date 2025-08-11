import { AttachmentBuilder, MessageFlags, SlashCommandSubcommandBuilder } from "discord.js"
import serverManager from "../../src/serverManager.js"

export default {
    data: new SlashCommandSubcommandBuilder()
        .setName("runcommand")
        .setDescription("Runs a command")
        .addStringOption(option => option.setName("command").setDescription("The command to run").setRequired(true)),

    permissionLevel: 2,

    async init() { },

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const command = interaction.options.getString("command")
        const response = await serverManager.sendRcon(serverInfo, command)

        if (response.status === "success") {
            if (response.message.length === 0) {
                return await interaction.reply({ content: "No response received", flags: MessageFlags.Ephemeral })
            }

            const rawMessage = response.message.replace(/§[0-9a-fk-or]/gi, "")
            const message = `\`\`\`\n${rawMessage}\n\`\`\``

            if (message.length <= 2000) {
                return await interaction.reply({ content: message, flags: MessageFlags.Ephemeral })
            }

            const buffer = Buffer.from(rawMessage, "utf-8")
            const attachment = new AttachmentBuilder(buffer, { name: "response.txt" })
            return await interaction.reply({ files: [attachment], flags: MessageFlags.Ephemeral })
        }

        await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
    }
}