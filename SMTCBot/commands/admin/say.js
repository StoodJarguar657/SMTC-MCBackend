import { MessageFlags, SlashCommandSubcommandBuilder } from "discord.js"
import sendMessage from "../../src/sendMessage.js"

export default {
    data: new SlashCommandSubcommandBuilder()
        .setName("say")
        .setDescription("Sends a admin message. No one would know who if multiple people are admins.")
        .addStringOption(option => option.setName("message").setDescription("The message to send").setRequired(true)),

    permissionLevel: 2,

    async init() { },

    /**
     * @param {import("discord.js").ChatInputCommandInteraction} interaction 
     * @param {import("serverInfo").ServerInfo} serverInfo 
     * @param {number} permissionLevel
     */
    async execute(interaction, serverInfo, permissionLevel) {
        const response = await sendMessage.adminSendMessage(serverInfo, interaction.options.getString("message"))
        if (response.status === "success") {
            return await interaction.reply({ content: "Sent message!", flags: MessageFlags.Ephemeral })
        }

        return await interaction.reply({ content: response.message, flags: MessageFlags.Ephemeral })
    }
}