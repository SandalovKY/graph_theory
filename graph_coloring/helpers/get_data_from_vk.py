import requests
import networkx
import matplotlib.pyplot as plt

accessToken = 'b103e067b103e067b103e06754b17f081abb103b103e067d39f1b52fc33651497904ef2'

def getFriendsIds(userId):
    friends_url = 'https://api.vk.com/method/friends.get?user_id={}&v=5.131&access_token={}'
    json_response = requests.get(friends_url.format(userId, accessToken)).json()
    if json_response.get('error'):
        print(json_response.get('error'))
        return list()
    return json_response['response']['items']

graph = {}
friend_ids = getFriendsIds(134310197)

for friend_id in friend_ids:
    graph[friend_id] = getFriendsIds(friend_id)


g = networkx.Graph()
for ind_i in graph:
    for ind_j in graph[ind_i]:
        if ind_i != ind_j and ind_i in friend_ids and ind_j in friend_ids:
            g.add_edge(ind_i, ind_j)

# networkx.write_adjlist(g, 'my_friends')
networkx.write_edgelist(g, 'my_friends_new')
# networkx.draw_networkx(g)
# plt.show()