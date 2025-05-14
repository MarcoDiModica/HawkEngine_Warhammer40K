using System;
using System.Collections.Generic;
using System.Linq;
using System.Numerics;

namespace HawkEngine
{
    public class Pathfinding
    {
        private const int gridSize = 32;
        public const float cellSize = 1f;
        private static readonly float DiagCost = (float)Math.Sqrt(2);

        private Vector3 gridOrigin;
        private bool[,] walkableGrid;
        private PriorityQueue<Node> openSet;
        private HashSet<(int, int)> closedSet;
        private Dictionary<(int, int), Node> nodeMap;

        public Pathfinding()
        {
            walkableGrid = new bool[gridSize, gridSize];
            gridOrigin = Vector3.Zero;
            openSet = new PriorityQueue<Node>();
            closedSet = new HashSet<(int, int)>();
            nodeMap = new Dictionary<(int, int), Node>();
        }

        public void UpdateGridOrigin(Vector3 origin)
        {
            gridOrigin = new Vector3(
                (float)Math.Floor(origin.X) - gridSize / 2,
                0,
                (float)Math.Floor(origin.Z) - gridSize / 2
            );

            for (int x = 0; x < gridSize; x++)
            {
                for (int y = 0; y < gridSize; y++)
                {
                    walkableGrid[x, y] = IsWalkable(x, y);
                }
            }
        }

        public List<Vector3> FindPath(Vector3 startW, Vector3 endW)
        {
            bool withinGrid = IsPointWithinGrid(startW) && IsPointWithinGrid(endW);
            if (!withinGrid)
            {
                UpdateGridOrigin(startW);
            }

            (int sx, int sy) = WorldToGrid(startW);
            (int tx, int ty) = WorldToGrid(endW);

            if (sx < 0 || sx >= gridSize || sy < 0 || sy >= gridSize ||
                tx < 0 || tx >= gridSize || ty < 0 || ty >= gridSize)
            {
                return null;
            }

            if (!walkableGrid[sx, sy])
                (sx, sy) = FindNearestWalkable(sx, sy);
            if (!walkableGrid[tx, ty])
                (tx, ty) = FindNearestWalkable(tx, ty);

            openSet.Clear();
            closedSet.Clear();
            nodeMap.Clear();

            Node startNode = new Node(sx, sy) { G = 0, H = Heuristic(sx, sy, tx, ty) };
            openSet.Enqueue(startNode, startNode.F);
            nodeMap[(sx, sy)] = startNode;

            while (openSet.Count > 0)
            {
                Node current = openSet.Dequeue();

                if (current.X == tx && current.Y == ty)
                    return SmoothPath(Retrace(current));

                closedSet.Add((current.X, current.Y));

                foreach (var (nx, ny) in GetNeighborCoords(current.X, current.Y))
                {
                    if (nx < 0 || nx >= gridSize || ny < 0 || ny >= gridSize)
                        continue;

                    if (closedSet.Contains((nx, ny)))
                        continue;

                    if (!walkableGrid[nx, ny])
                        continue;

                    float tentativeG = current.G + ((nx - current.X != 0 && ny - current.Y != 0) ? DiagCost : 1f);

                    if (!nodeMap.TryGetValue((nx, ny), out Node neighbor))
                    {
                        neighbor = new Node(nx, ny)
                        {
                            G = tentativeG,
                            H = Heuristic(nx, ny, tx, ty),
                            Parent = current
                        };
                        nodeMap[(nx, ny)] = neighbor;
                        openSet.Enqueue(neighbor, neighbor.F);
                    }
                    else if (tentativeG < neighbor.G)
                    {
                        neighbor.G = tentativeG;
                        neighbor.Parent = current;
                        openSet.UpdatePriority(neighbor, neighbor.F);
                    }
                }
            }

            return null;
        }

        private bool IsPointWithinGrid(Vector3 point)
        {
            (int x, int y) = WorldToGrid(point);
            return x >= 0 && x < gridSize && y >= 0 && y < gridSize;
        }

        private (int, int) FindNearestWalkable(int x, int y)
        {
            bool[,] visited = new bool[gridSize, gridSize];
            Queue<(int, int)> queue = new Queue<(int, int)>();

            queue.Enqueue((x, y));
            visited[x, y] = true;

            while (queue.Count > 0)
            {
                var (cx, cy) = queue.Dequeue();

                if (walkableGrid[cx, cy])
                    return (cx, cy);

                foreach (var (nx, ny) in GetNeighborCoords(cx, cy))
                {
                    if (nx < 0 || nx >= gridSize || ny < 0 || ny >= gridSize)
                        continue;

                    if (visited[nx, ny])
                        continue;

                    visited[nx, ny] = true;
                    queue.Enqueue((nx, ny));
                }
            }

            return (x, y);
        }

        private IEnumerable<(int, int)> GetNeighborCoords(int x, int y)
        {
            for (int dx = -1; dx <= 1; dx++)
            {
                for (int dy = -1; dy <= 1; dy++)
                {
                    if (dx == 0 && dy == 0)
                        continue;

                    if (dx != 0 && dy != 0)
                    {
                        if (!IsWalkable(x + dx, y) || !IsWalkable(x, y + dy))
                            continue;
                    }

                    yield return (x + dx, y + dy);
                }
            }
        }

        private (int, int) WorldToGrid(Vector3 worldPos)
        {
            int gx = (int)Math.Floor(worldPos.X - gridOrigin.X);
            int gy = (int)Math.Floor(worldPos.Z - gridOrigin.Z);

            return (gx, gy);
        }

        private Vector3 GridToWorld(int x, int y)
        {
            return new Vector3(
                gridOrigin.X + x + 0.5f,
                0.5f,
                gridOrigin.Z + y + 0.5f
            );
        }

        private bool IsWalkable(int x, int y)
        {
            return true;
            if (x < 0 || x >= gridSize || y < 0 || y >= gridSize)
                return false;

            Vector3 center = GridToWorld(x, y);
            float r = cellSize * 0.5f;

            var hits = Physics.OverlapSphere(center, r, "Collider") ?? Array.Empty<GameObject>();
            return !hits.Any(go => go != null && go.tag == "Collider");
        }

        private static float Heuristic(int x1, int y1, int x2, int y2)
        {
            int dx = Math.Abs(x2 - x1);
            int dy = Math.Abs(y2 - y1);

            return (dx + dy) + (DiagCost - 2f) * Math.Min(dx, dy);
        }

        private List<Vector3> Retrace(Node end)
        {
            var path = new List<Vector3>();

            for (Node curr = end; curr != null; curr = curr.Parent)
            {
                path.Add(GridToWorld(curr.X, curr.Y));
            }

            path.Reverse();
            return path;
        }

        private List<Vector3> SmoothPath(List<Vector3> path)
        {
            if (path == null || path.Count <= 2)
                return path;

            var smoothed = new List<Vector3> { path[0] };
            int current = 0;

            while (current < path.Count - 1)
            {
                int furthest = current + 1;

                for (int i = furthest + 1; i < path.Count; i++)
                {
                    if (IsPathClear(path[current], path[i]))
                    {
                        furthest = i;
                    }
                }

                smoothed.Add(path[furthest]);
                current = furthest;
            }

            return smoothed;
        }

        private bool IsPathClear(Vector3 start, Vector3 end)
        {
            Vector3 dir = end - start;
            float dist = dir.Length();
            dir = Vector3.Normalize(dir);

            int steps = Math.Max(1, (int)(dist / (cellSize * 0.5f)));

            for (int i = 1; i < steps; i++)
            {
                Vector3 point = start + dir * (dist * i / steps);
                (int x, int y) = WorldToGrid(point);

                if (x < 0 || x >= gridSize || y < 0 || y >= gridSize || !walkableGrid[x, y])
                    return false;
            }

            return true;
        }

        private class Node
        {
            public int X, Y;
            public float G = float.MaxValue;
            public float H;
            public Node Parent;
            public float F => G + H;

            public Node(int x, int y)
            {
                X = x;
                Y = y;
            }
        }
    }

    public class PriorityQueue<T> where T : class
    {
        private List<(T item, float priority)> elements = new List<(T, float)>();
        private Dictionary<T, int> indices = new Dictionary<T, int>();

        public int Count => elements.Count;

        public void Clear()
        {
            elements.Clear();
            indices.Clear();
        }

        public void Enqueue(T item, float priority)
        {
            elements.Add((item, priority));
            indices[item] = elements.Count - 1;
            BubbleUp(elements.Count - 1);
        }

        public T Dequeue()
        {
            if (elements.Count == 0)
                throw new InvalidOperationException("Queue is empty");

            T item = elements[0].item;
            indices.Remove(item);

            if (elements.Count > 1)
            {
                elements[0] = elements[elements.Count - 1];
                indices[elements[0].item] = 0;
                elements.RemoveAt(elements.Count - 1);
                BubbleDown(0);
            }
            else
            {
                elements.Clear();
            }

            return item;
        }

        public void UpdatePriority(T item, float priority)
        {
            if (!indices.TryGetValue(item, out int index))
                return;

            float oldPriority = elements[index].priority;
            elements[index] = (item, priority);

            if (priority < oldPriority)
                BubbleUp(index);
            else if (priority > oldPriority)
                BubbleDown(index);
        }

        private void BubbleUp(int index)
        {
            while (index > 0)
            {
                int parent = (index - 1) / 2;

                if (elements[index].priority >= elements[parent].priority)
                    break;

                Swap(index, parent);
                index = parent;
            }
        }

        private void BubbleDown(int index)
        {
            while (true)
            {
                int left = index * 2 + 1;
                int right = index * 2 + 2;
                int smallest = index;

                if (left < elements.Count && elements[left].priority < elements[smallest].priority)
                    smallest = left;

                if (right < elements.Count && elements[right].priority < elements[smallest].priority)
                    smallest = right;

                if (smallest == index)
                    break;

                Swap(index, smallest);
                index = smallest;
            }
        }

        private void Swap(int a, int b)
        {
            (T itemA, float priorityA) = elements[a];
            (T itemB, float priorityB) = elements[b];

            elements[a] = (itemB, priorityB);
            elements[b] = (itemA, priorityA);

            indices[itemA] = b;
            indices[itemB] = a;
        }
    }
}