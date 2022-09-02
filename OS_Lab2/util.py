import pickle


def serialize(arg):
    return pickle.dumps(arg)

def deserialize(data):
    return pickle.loads(data)