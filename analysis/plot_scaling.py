import matplotlib.pyplot as plt

# Replace with your actual measured numbers
events = [500000, 5000000, 20000000]

manual_eps = [98467.5, 102538, 100553]
rdf_eps = [76490.5, 95132.5, 96818.3]

plt.figure()
plt.plot(events, manual_eps, marker='o', label="Manual")
plt.plot(events, rdf_eps, marker='o', label="RDataFrame")

plt.xscale("log")
plt.xlabel("Number of Events")
plt.ylabel("Events per Second")
plt.title("Scaling Study: Manual vs RDataFrame")
plt.legend()

plt.tight_layout()
plt.savefig("scaling.png")
plt.show()
