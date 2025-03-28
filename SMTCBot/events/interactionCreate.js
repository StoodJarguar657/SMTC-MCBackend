const CommandManager = require("../src/managers/CommandManager")
const UserManager = require("../src/managers/UserManager")
const ModalManager = require("../src/managers/ModalManager")

/**
 * @param {import("discord.js").Interaction} interaction 
 */
module.exports = async (interaction) => {
    if(interaction.member && interaction.member.id) {
        const userId = BigInt(interaction.member.id)

        if(!await UserManager.UserExists(userId))
            await UserManager.CreateUser(userId)
    }

    if(interaction.isCommand()) {
        await CommandManager.RunCommand(interaction)
        return
    }

    if(interaction.isModalSubmit()) {
        await ModalManager.RunModal(interaction)
        return
    }
}