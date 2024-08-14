import matplotlib.pyplot as plt
import numpy as np
import os
from datetime import datetime


def main():
    # Compression 
    data = ['Compression Fraction', 'Rate Accuracy', 'Depth Accuracy', 'Recoil Accuracy']
    feedback = [98.50, 49.00, 96.00, 100.00]
    no_feedback = [78.00, 45.00, 83.50, 100.00]

    # Hands Off 
    hands_feedback = [1.0]
    hands_no_feedback = [0.5]

    # Reduced bar width
    bar_width = 0.25
    bar_width_hands = 0.05

    x = np.arange(len(data))
    x_hands = np.arange(1)

    # Adjust figure size (width x height in inches)
    fig, axs = plt.subplots(1, 2, figsize=(12, 4))

    # Plotting the bars with reduced space
    axs[0].bar(x - bar_width/2, no_feedback, bar_width, label='No Feedback Device')
    axs[0].bar(x + bar_width/2, feedback, bar_width, label='Feedback Device')

    # Compressions
    axs[0].set_xlabel('Compression Quality Variables')
    axs[0].set_ylabel('Accuracy (% of Compressions)')
    axs[0].set_title('Effect of Multimodal Feedback on CPR Performance')
    axs[0].set_xticks(x)
    axs[0].set_xticklabels(data)
    axs[0].legend(loc='lower right')
    axs[0].tick_params('x', labelsize=8)
    axs[0].set_ylim(40, 105)

    # # Hands Off Time
    # axs[1].bar(x_hands - bar_width_hands/2, hands_no_feedback, bar_width, label='No Feedback Device')
    # axs[1].bar(x_hands + bar_width_hands/2, hands_feedback, bar_width, label='Feedback Device')
    # axs[1].set_xlabel('Hands Off Trial')
    # axs[1].set_ylabel('Time (s)')
    # axs[1].set_title('Hands Off Time with Feedback Device')
    # axs[1].legend(loc='lower right')
    # axs[1].tick_params('x', labelsize=8)

    output_directory = "C:\\Users\\allan\\Documents\\cpr-feedback\\lib\\participant_data\\feedback_analysis\\m{}-d{}-y{}".format(
        datetime.now().month, datetime.now().day, datetime.now().year)

    os.makedirs(output_directory, exist_ok=True)

    output_path = os.path.join(output_directory, 'm{}-d{}.png'.format(datetime.now().month, datetime.now().day))
    plt.savefig(output_path)


    plt.show()


if __name__ == "__main__":
    main()
