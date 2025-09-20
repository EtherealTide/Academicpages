---
title: "A Cross-Scenario GNSS Spoofing Detection Method Based on Transfer Learning"
collection: publications
category: conferences
permalink: /publication/2025-09-30-transfer-learning-trustworthy-PNT
excerpt: 'This paper employed a time-weighted step-output Transformer architecture, pre-trained on data with similar deception patterns in static scenarios. Subsequently, transfer learning techniques are applied for fine-tuning under conditions of scarce dynamic scenario data, further enhancing classification accuracy to 92.98%. This demonstrates a significant advantage over classical models (such as 1D-CNN, GRU, LSTM, and RNN), effectively addressing the challenge of deception interference detection in data-scarce scenarios.'
date: 2025-09-30
venue: 'Institute of Navigation-Global Navigation Satellite Systems Plus Conference(ION GNSS+)'
slidesurl: # 'http://academicpages.github.io/files/slides1.pdf'
paperurl: #'https://doi.org/10.1109/ICPICS62053.2024.10796207'
citation: 'EtherealTide (2024). &quot;QUBO Model-based Resource Allocation Optimization for Distributed Computing Systems.&quot; <i>IEEE ICPICS 2024</i>. DOI: 10.1109/ICPICS62053.2024.10796207'
---

## Abstract

As a critical information infrastructure, the reliability of positioning, navigation, and timing (PNT) services from Global Navigation Satellite Systems (GNSS) is directly related to national security and economic operation. However, the open natureof satellite signals and significant propagation path loss make them susceptible to spoofing attacks. Existing spoofing detection methods exhibit poor cross-scenario generalization, causing performance degradation with scarce labeled data and high false alarm rates in multipath environments. To address these issues, we propose a cross-scenario detection method that employs a 
Transformer to capture long-term signal dynamics and a pre-training/fine-tuning transfer learning framework to ensure rapid adaptation to new data domains. Experimental results demonstrate that the proposed transfer learning framework improve detection accuracy by up to 10.54% and reduce training time by 75.6% using only 5% of labeled data from a new domain. Furthermore, the model demonstrates high robustness in complex scenarios, achieving a 99.75% detection rate against a 0.03% false alarm rate in concurrent spoofing and multipath environments. This work provides a highly robust solution with cross scenario adaptability for anti-spoofing protection in complex electromagnetic environments.

## Key Contributions
1. The Transformer architecture is utilized in place of traditional CNNs. By harnessing the self-attention mechanism, it captures the complete temporal dependency relationships of signals, effectively addressing the limitations of local convolutional kernels that struggle to model the sudden, phased changes typical of spoofing signals. This approach effectively differentiates the continuous fluctuations of multipath signals from the temporal anomalies associated with spoofing signals.
2. Additionally, learnable weight parameters are incorporated at the output layer of the Transformer to dynamically enhance the feature representation of key phases in spoofing signals, such as the injection and completion phases. This approach improves the model's sensitivity to temporal turning points and enhances its ability to discriminate between artificially manipulated features.
3. To tackle the challenges of data scarcity and distribution discrepancies in real-world situations, we propose a hierarchical transfer strategy. This approach involves freezing the parameters of the lower layers of the Transformer to maintain its general temporal representation capabilities, while dynamically fine-tuning the weighted modules and classifier in the higher layers. This enables the model to adapt quickly to new scenarios.

[Download-Paper]({{ site.url }}{{ site.baseurl }}/files/ION GNSS+ A Cross-Scenario GNSS Spoofing Detection Method Based on Transfer Learning.pdf){:download="ION GNSS+ A Cross-Scenario GNSS Spoofing Detection Method Based on Transfer Learning.pdf"}


